use crate::{watcher::Watcher};
use sysinfo::{CpuExt, SystemExt};

pub fn detect_crypto_activity(watcher: &mut Watcher) {
    watcher.system_info.refresh_cpu();
    let mut avrg_load = 0.0;
    for cpu in watcher.system_info.cpus() {
        avrg_load += cpu.cpu_usage();
    }
    avrg_load /= watcher.system_info.cpus().len() as f32;
    if avrg_load > 90.0 {
        println!("Intense cpu activiy detected: {}%!", avrg_load);
    }
}