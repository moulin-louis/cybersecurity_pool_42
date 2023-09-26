use crate::{watcher::Watcher};
use sysinfo::{CpuExt, SystemExt};

pub fn detect_crypto_activity(watcher: &mut Watcher) {
    watcher.system_info.refresh_cpu();
    let mut avrg_load: u32 = 0;
    let cpus = watcher.system_info.cpus();
    for cpu in cpus {
        avrg_load += cpu.cpu_usage() as u32;
    }
    avrg_load /= cpus.len() as u32;
    println!("Avrg loaf is {}", avrg_load);
    if avrg_load > 90 {
        println!("Intense cpu activiy detected: {}%!", avrg_load);
    }
}