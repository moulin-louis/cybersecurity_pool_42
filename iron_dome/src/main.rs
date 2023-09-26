mod watcher;
use watcher::Watcher;
mod entropy_check;
use entropy_check::detect_entropy_change;
mod read_abuse;
use read_abuse::detect_disk_read_abuse;
mod crypto_activity;
use crypto_activity::detect_crypto_activity;
use std::{
    env,
    thread,
    fs::File,
    time::Duration,
};
use std::io::Write;
use std::sync::Arc;
use std::sync::atomic::{AtomicBool, Ordering};
use std::sync::atomic::Ordering::SeqCst;
use daemonize::Daemonize;
use sysinfo::{System, SystemExt};
use ctrlc::*;
use alloc_counter::{count_alloc, AllocCounterSystem};
#[global_allocator]
static A: AllocCounterSystem = AllocCounterSystem;


const LOG_DIR: &str = " /var/log/irondome/irondome.log";
const LOG_DIR_ERR: &str = " /var/log/irondome/irondome_err.log";
const TTS:Duration = Duration::from_secs(2);


fn init_daemon() -> Option<()> {
    let log_file: File = match File::create(LOG_DIR) {
        Ok(val) => val,
        Err(_) => return None,
    };
    let log_file_err: File = match File::create(LOG_DIR_ERR){
        Ok(val) => val,
        Err(_) => return None,
    };
    let daemonize: Daemonize<()> = Daemonize::new()
        .pid_file("/tmp/test.pid")
        .working_directory("/tmp")
        .stderr(log_file_err)
        .stdout(log_file);
    match daemonize.start() {
        Ok(_) => {
            Some(())
        },
        Err(err) => {
            eprintln!("Failed to launch daemon: {}", err);
            None
        },
    }
}

fn main() {
    let running = Arc::new(AtomicBool::new(true));
    let r = running.clone();
    set_handler(move || {
        println!("Ctrl-c received");
        r.store(false, Ordering::SeqCst); }).unwrap();

    if !System::IS_SUPPORTED {
        eprintln!("{}", "SystemExt is not supported".to_ascii_uppercase());
        return;
    }
    let (allocs, deallocs) = count_alloc(|| {
        let v: Vec<u8> = Vec::with_capacity(1024);
        v
    });

    let mut daemon_mode: bool = true;
    let mut watcher: Watcher = Watcher::default();
    watcher.system_info.refresh_cpu();
    if watcher.path_to_watch.contains(&"--no-daemon".to_string()) {
        let index: usize = watcher.path_to_watch.iter().position(|x| x == "--no-daemon").unwrap();
        watcher.path_to_watch.remove(index);
        daemon_mode = false;
    }
    if daemon_mode && init_daemon().is_none() {
        return;
    }
    if watcher.path_to_watch.is_empty() {
        println!("No path provided, default to $HOME");
        watcher.path_to_watch.push(env::var("HOME").expect("Cant find HOME env var"));
    }
    while running.load(SeqCst) {
        detect_entropy_change(&mut watcher);
        detect_disk_read_abuse(&mut watcher);
        detect_crypto_activity(&mut watcher);
        std::io::stdout().flush().unwrap();
        thread::sleep(TTS);
    }
    println!("Allocations: {:?}", allocs);
    println!("Deallocations: {:?}", deallocs);
}