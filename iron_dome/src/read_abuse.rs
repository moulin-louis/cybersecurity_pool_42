use std::path::PathBuf;
use crate::watcher::Watcher;
use sysinfo::{ProcessExt, SystemExt, DiskUsage, Pid, PidExt};
use crate::read_file;

const THRESHOLD_READ: u64 = 10000000;

fn check_abuse(pid: &Pid, disk_usage: &DiskUsage) {
    if disk_usage.read_bytes > THRESHOLD_READ {
        let mut path_pid = String::from("/proc/");
        path_pid.push_str(pid.as_u32().to_string().as_str());
        path_pid.push_str("/comm");
        let name_process = read_file(PathBuf::from(path_pid)).unwrap();
        let mut name_process = String::from_utf8(name_process.clone()).unwrap();
        name_process.remove(name_process.len() - 1);
        println!("Potential read abuse : [{}]:[{}] -> {} bytes read",pid, name_process,disk_usage.read_bytes);
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