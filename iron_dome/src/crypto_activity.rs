use std::time::UNIX_EPOCH;
use crate::{watcher::Watcher};
use sysinfo::{CpuExt, SystemExt};

pub fn detect_crypto_activity(watcher: &mut Watcher) {
    watcher.system_info.refresh_cpu();
    let mut avrg_load: u32 = 0;
    for cpu in watcher.system_info.cpus() {
        avrg_load += cpu.cpu_usage() as u32;
    }
    println!("{}%", avrg_load / watcher.system_info.cpus().len() as u32);
    avrg_load /= watcher.system_info.cpus().len() as u32;
    if avrg_load > 90 {
        println!("Intense cpu activiy detected: {}%!", avrg_load);
    }
}