use sysinfo::{CpuExt, SystemExt, System};

pub fn detect_crypto_activity(system_info: &mut System) {
    let mut avrg_load: u32 = 0;
    for cpu in system_info.cpus() {
        avrg_load += cpu.cpu_usage() as u32;
    }
    avrg_load /= system_info.cpus().len() as u32;
    if avrg_load > 90 {
        println!("Intense cpu activiy detected: {}%!", avrg_load);
    }
}