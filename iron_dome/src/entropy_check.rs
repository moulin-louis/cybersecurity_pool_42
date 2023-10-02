use crate::watcher::Watcher;
use inline_colorization::color_red;
use std::{
    fs::{read_dir, ReadDir},
    path::PathBuf,
    sync::{atomic::AtomicBool, Arc, Mutex, MutexGuard},
};

fn updating_files_to_watch(watcher: &mut Watcher) {
    let mut to_delete: Vec<String> = Vec::new();
    for index in 0..watcher.path_to_watch.len() {
        let path: &String = &watcher.path_to_watch[index];
        let list_files: ReadDir = match read_dir(path) {
            Ok(value) => value,
            Err(err) => {
                eprintln!("{color_red}ERROR: Error reading files of {}: {}", path, err);
                to_delete.push(path.clone());
                continue;
            }
        };
        for dir_entry in list_files {
            match dir_entry {
                Err(err) => {
                    eprintln!("{color_red}ERROR: Error read path: {}", err);
                }
                Ok(val) => {
                    if val.file_type().unwrap().is_dir() {
                        continue;
                    }
                    watcher.file_watched.entry(val.path()).or_insert(0.0);
                }
            }
        }
    }
    for path in to_delete {
        let pos: usize = watcher
            .path_to_watch
            .iter()
            .position(|x: &String| x == &path)
            .unwrap();
        watcher.path_to_watch.remove(pos);
    }
}

fn update_entropy_file_to_watch(watcher: &mut Watcher, flag_arc: &mut Arc<Mutex<[AtomicBool; 3]>>) {
    let mut to_delete: Vec<PathBuf> = Vec::new();
    let mut found_file_sus: bool = false;
    for (key, _val) in watcher.file_watched.clone() {
        match watcher.update_entropy(&key, flag_arc) {
            None => {
                eprintln!("{color_red}ERROR: files_to_watch change, aborting current loop");
                to_delete.push(key.clone());
                break;
            }
            Some(val) => {
                if val == 2 {
                    found_file_sus = true;
                }
            }
        };
    }
    for file in to_delete {
        watcher.file_watched.remove(&file);
    }
    if !found_file_sus {
        let mut flags: MutexGuard<'_, [AtomicBool; 3]> = flag_arc.lock().unwrap();
        flags[0] = AtomicBool::new(false);
    }
}

pub fn detect_entropy_change(watcher: &mut Watcher, flag_arc: &mut Arc<Mutex<[AtomicBool; 3]>>) {
    updating_files_to_watch(watcher);
    update_entropy_file_to_watch(watcher, flag_arc);
}
