use std::{
    borrow::Cow,
    process::{Command, Output},
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

pub fn detect_crypto_activity(system_info: &mut System) {
    system_info.refresh_all();
    system_info.refresh_cpu();
    for process in system_info.processes().values() {
        let output: Output = match Command::new("ldd").arg(process.exe()).output() {
            Ok(val) => val,
            Err(err) => {
                println!("Cant launch command ldd: {}", err);
                continue;
            }
        };
        let libraries: Cow<str> = String::from_utf8_lossy(&output.stdout);
        if check_linked_lib(&libraries)
            && process.cpu_usage() > (100.0 / system_info.cpus().len() as f32)
        {
            println!(
                "Process: {}, potential high cryptographic activity",
                process.name()
            );
        }
    }
}
