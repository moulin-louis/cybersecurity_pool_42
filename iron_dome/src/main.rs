use std::{
    collections::HashMap,
    env,
    fs::File,
    sync::{
        atomic::{AtomicBool, Ordering::SeqCst},
        Arc, Mutex, MutexGuard,
    },
    thread,
    thread::JoinHandle,
    time::Duration,
};

mod watcher;
use watcher::Watcher;
mod entropy_check;
use entropy_check::detect_entropy_change;
mod read_abuse;
use read_abuse::detect_disk_read_abuse;
mod crypto_activity;
use crypto_activity::detect_crypto_activity;
use ctrlc::set_handler;
use daemonize::Daemonize;
use sysinfo::{System, SystemExt};

const LOG_DIR: &str = " /var/log/irondome/irondome.log";
const LOG_DIR_ERR: &str = " /var/log/irondome/irondome_err.log";
const TTS: Duration = Duration::from_secs(2);

fn init_daemon() -> Option<()> {
    let log_file: File = match File::create(LOG_DIR) {
        Ok(val) => val,
        Err(_) => {
            eprintln!("Impossible to create: {}", LOG_DIR);
            return None;
        }
    };
    let log_file_err: File = match File::create(LOG_DIR_ERR) {
        Ok(val) => val,
        Err(_) => {
            eprintln!("Impossible to create: {}", LOG_DIR_ERR);
            return None;
        }
    };
    let daemonize: Daemonize<()> = Daemonize::new()
        .pid_file("/tmp/test.pid")
        .working_directory("/tmp")
        .stderr(log_file_err)
        .stdout(log_file);
    match daemonize.start() {
        Ok(_) => Some(()),
        Err(err) => {
            eprintln!("Failed to launch daemon: {}", err);
            None
        }
    }
}

fn main() {
    let mut daemon_mode: bool = true;
    if env::args().any(|x: String| x == "--no-daemon") {
        println!("Running in foreground mode");
        daemon_mode = false;
    }
    if daemon_mode && init_daemon().is_none() {
        return;
    }
    let ranson_flag: Arc<Mutex<[AtomicBool; 3]>> = Arc::new(Mutex::new([
        AtomicBool::new(false),
        AtomicBool::new(false),
        AtomicBool::new(false),
    ]));
    let running: Arc<AtomicBool> = Arc::new(AtomicBool::new(true));
    let r: Arc<AtomicBool> = running.clone();
    set_handler(move || {
        println!("Ctrl-c received");
        r.clone().store(false, SeqCst);
    })
    .unwrap();

    let thread_entro: JoinHandle<()> = thread::spawn({
        let running: Arc<AtomicBool> = running.clone();
        let mut flag: Arc<Mutex<[AtomicBool; 3]>> = ranson_flag.clone();
        move || {
            let mut watcher: Watcher = Watcher::default();
            if watcher.path_to_watch.is_empty() {
                println!("No path provided, default to $HOME");
                println!("Next time we advise you to provide path with your important file");
                watcher
                    .path_to_watch
                    .push(env::var("HOME").expect("Cant find HOME env key"));
            }
            if !daemon_mode {
                let index: usize = watcher
                    .path_to_watch
                    .iter()
                    .position(|x: &String| x == "--no-daemon")
                    .unwrap();
                watcher.path_to_watch.remove(index);
            }
            while running.load(SeqCst) {
                detect_entropy_change(&mut watcher, &mut flag);
                thread::sleep(TTS);
            }
        }
    });

    let thread_crypto: JoinHandle<()> = thread::spawn({
        let running: Arc<AtomicBool> = running.clone();
        let mut flag: Arc<Mutex<[AtomicBool; 3]>> = ranson_flag.clone();
        move || {
            let mut system_info: System = System::new();
            system_info.refresh_all();
            while running.load(SeqCst) {
                detect_crypto_activity(&mut system_info, &mut flag);
                thread::sleep(TTS);
            }
        }
    });

    let thread_disk: JoinHandle<()> = thread::spawn({
        let running: Arc<AtomicBool> = running.clone();
        let mut flag: Arc<Mutex<[AtomicBool; 3]>> = ranson_flag.clone();
        move || {
            let mut disk_info: HashMap<String, u64> = HashMap::new();
            while running.load(SeqCst) {
                detect_disk_read_abuse(&mut disk_info, &mut flag);
                thread::sleep(TTS);
            }
        }
    });

    let thread_check: JoinHandle<()> = thread::spawn({
        let running: Arc<AtomicBool> = running;
        let flag: Arc<Mutex<[AtomicBool; 3]>> = ranson_flag;
        move || {
            while running.load(SeqCst) {
                {
                    let flags: MutexGuard<'_, [AtomicBool; 3]> = flag.lock().unwrap();
                    println!("Entropy flag:\t{:?}", flags[0]);
                    println!("Disk flag:\t{:?}", flags[1]);
                    println!("Crypto flag:\t{:?}", flags[2]);
                    println!();
                }
                thread::sleep(TTS);
            }
        }
    });

    thread_entro.join().unwrap();
    thread_crypto.join().unwrap();
    thread_disk.join().unwrap();
    thread_check.join().unwrap();
}
