use inline_colorization::{color_red, color_yellow};
use std::{
    borrow::Cow,
    process::{Command, Output},
    sync::{atomic::AtomicBool, Arc, Mutex, MutexGuard},
};
use sysinfo::{ProcessExt, System, SystemExt};

fn check_linked_lib(libraries: &str) -> bool {
    if libraries.contains("libssl")
        || libraries.contains("libcrypto")
        || libraries.contains("libgcrypt")
        || libraries.contains("libsodium")
        || libraries.contains("libgpg")
        || libraries.contains("libbotan")
        || libraries.contains("libmbedtls")
        || libraries.contains("libwolfssl")
        || libraries.contains("libnss3")
    {
        return true;
    }
    false
}

pub fn detect_crypto_activity(
    system_info: &mut System,
    flag_arc: &mut Arc<Mutex<[AtomicBool; 3]>>,
) {
    let mut found_process_sus: bool = false;
    system_info.refresh_all();
    system_info.refresh_cpu();
    for process in system_info.processes().values() {
        let output: Output = match Command::new("ldd").arg(process.exe()).output() {
            Ok(val) => val,
            Err(err) => {
                eprintln!("{color_red}ERROR: Cant launch command ldd: {}", err);
                continue;
            }
        };
        let libraries: Cow<str> = String::from_utf8_lossy(&output.stdout);
        if check_linked_lib(&libraries)
            && process.cpu_usage() > (100.0 / system_info.cpus().len() as f32)
        {
            found_process_sus = true;
            //flaging crypto for all thread
            let mut flags: MutexGuard<'_, [AtomicBool; 3]> = flag_arc.lock().unwrap();
            flags[2] = AtomicBool::new(true);
            println!(
                "{color_yellow}WARNING: Process: {}, potential high cryptographic activity",
                process.exe().display()
            );
        }
    }
    if !found_process_sus {
        let mut flags: MutexGuard<'_, [AtomicBool; 3]> = flag_arc.lock().unwrap();
        flags[2] = AtomicBool::new(false);
    }
}
