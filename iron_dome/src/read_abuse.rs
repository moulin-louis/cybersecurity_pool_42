use std::{path::PathBuf, process};
use std::fs::read;
use crate::watcher::Watcher;
use sysinfo::{ProcessExt, SystemExt, Pid, PidExt};

const THRESHOLD_READ: u64 = 1000000; //1MB

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

fn check_abuse(pid: &Pid, new_value: u64, old_value: u64) {
    let pid_nbr: u32 = pid.as_u32();
    if process::id() == pid_nbr{
        return;
    }
    let read_since_epoch = new_value - old_value;
    if read_since_epoch > THRESHOLD_READ {
        let mut path_pid: String = String::from("/proc/");
        path_pid.push_str(pid_nbr.to_string().as_str());
        path_pid.push_str("/comm");
        let mut name_process: String = String::from_utf8(read_file(PathBuf::from(path_pid)).unwrap()).unwrap();
        name_process.remove(name_process.len() - 1);
        println!("Potential read abuse : [{}]:[{}] -> {} bytes read",pid, name_process, read_since_epoch);
    }
}

pub fn detect_disk_read_abuse(watcher: &mut Watcher) {
    watcher.system_info.refresh_processes();
    watcher.system_info.refresh_disks();
    for (pid, process) in watcher.system_info.processes() {
        if watcher.process_read.contains_key(pid) {
            let old_value: u64 = *watcher.process_read.get(pid).unwrap();
            *watcher.process_read.get_mut(pid).unwrap() = process.disk_usage().total_read_bytes;
            check_abuse(pid, *watcher.process_read.get(pid).unwrap(), old_value);
        } else {
            watcher.process_read.insert(*pid, process.disk_usage().total_read_bytes);
        }
    }
}