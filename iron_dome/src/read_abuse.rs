use crate::watcher::Watcher;
use sysinfo::{ProcessExt, SystemExt, DiskUsage, Pid};

const THRESHOLD_READ: u64 = 10000000;

fn check_abuse(pid: &Pid, disk_usage: &DiskUsage) {
    if disk_usage.read_bytes > THRESHOLD_READ {
        println!("Potential read abuse : [{}] -> {} bytes read",pid, disk_usage.read_bytes);
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