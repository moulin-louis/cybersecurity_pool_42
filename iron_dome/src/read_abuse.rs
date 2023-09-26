use std::{path::PathBuf, process};
use std::fs::read;
use crate::watcher::Watcher;
use sysinfo::{ProcessExt, SystemExt, DiskUsage, Pid, PidExt};

const THRESHOLD_READ: u64 = 100000000; //100MB

fn read_file(path: PathBuf) -> Result<Vec<u8>, i32> {
    match read(&path) {
        Ok(val) => Ok(val),
        Err(err) => {
            if err.raw_os_error().unwrap() == 2 {
                return Err(2);
            }
            eprintln!("Error reading {:?} : {}" ,path, err);
            Err(1)
        }
    }
}

fn check_abuse(pid: &Pid, disk_usage: &DiskUsage) {
    let pid_nbr: u32 = pid.as_u32();
    if process::id() == pid_nbr{
        return;
    }
    let mut path_pid: String = String::from("/proc/");
    path_pid.push_str(pid_nbr.to_string().as_str());
    path_pid.push_str("/comm");
    let mut name_process: String = String::from_utf8(read_file(PathBuf::from(path_pid)).unwrap()).unwrap();
    if name_process.contains("python3") {
        println!("read_bytes of {} = {}", name_process, disk_usage.read_bytes);
        println!("total = read_bytes of {} = {}", name_process, disk_usage.total_written_bytes);
    }
    if disk_usage.read_bytes > THRESHOLD_READ {
        name_process.remove(name_process.len() - 1);
        println!("Potential read abuse : [{}]:[{}] -> {} bytes read",pid, name_process, disk_usage.read_bytes);
    }
}

pub fn detect_disk_read_abuse(watcher: &mut Watcher) {
    watcher.system_info.refresh_processes();
    watcher.system_info.refresh_disks();
    for (pid, process) in watcher.system_info.processes() {
        let disk_usage: DiskUsage = process.disk_usage();
        check_abuse(pid, &disk_usage);
    }
}