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
    fs::{File, read},
    time::Duration,
    path::PathBuf,
};
use daemonize::Daemonize;

const LOG_DIR: &str = " /var/log/irondome/irondome.log";
const LOG_DIR_ERR: &str = " /var/log/irondome/irondome_err.log";
const TTS:Duration = Duration::from_secs(2);

pub fn read_file(path: PathBuf) -> Result<Vec<u8>, i32> {
    match read(&path) {
        Ok(val) => Ok(val),
        Err(err) => {
            if err.raw_os_error().unwrap() == 2 {
                return Err(2);
            }
            eprintln!("Error reading {:?} : {}" ,path, err);
            Err(1)
        }
    }
}

fn init_daemon() -> Option<()> {
    let log_file = match File::create(LOG_DIR) {
        Ok(val) => val,
        Err(_) => return None,
    };
    let log_file_err = match File::create(LOG_DIR_ERR){
        Ok(val) => val,
        Err(_) => return None,
    };
    let daemonize = Daemonize::new()
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
    let mut daemon_mode: bool = true;
    let mut watcher = Watcher::default();
    if watcher.path_to_watch.contains(&"--no-daemon".to_string()) {
        let index = watcher.path_to_watch.iter().position(|x| x == "--no-daemon").unwrap();
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
    println!("Starting monitoring");
    loop {
        detect_entropy_change(&mut watcher);
        detect_disk_read_abuse(&mut watcher);
        detect_crypto_activity(&mut watcher);
        thread::sleep(TTS);
    }
    eprintln!("Somehow we are here, not suppose to happen, weird");
}