use crate::watcher::Watcher;
use std::{
    fs::{read_dir, ReadDir},
    path::PathBuf,
};

fn updating_files_to_watch(watcher: &mut Watcher) {
    let mut to_delete: Vec<String> = Vec::new();
    for index in 0..watcher.path_to_watch.len() {
        let path: &String = &watcher.path_to_watch[index];
        let list_files: ReadDir = match read_dir(path) {
            Ok(value) => value,
            Err(err) => {
                eprintln!("Error reading files of {}: {}", path, err);
                to_delete.push(path.clone());
                continue;
            }
        };
        for dir_entry in list_files {
            match dir_entry {
                Err(err) => {
                    eprintln!("Error read path: {}", err);
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

fn update_entropy_file_to_watch(watcher: &mut Watcher) {
    let mut to_delete: Vec<PathBuf> = Vec::new();
    for (key, _val) in watcher.file_watched.clone() {
        if watcher.update_entropy(&key).is_none() {
            eprintln!("files_to_watch change, aborting current loop");
            to_delete.push(key.clone());
        }
    }
    for file in to_delete {
        watcher.file_watched.remove(&file);
    }
}

pub fn detect_entropy_change(watcher: &mut Watcher) {
    updating_files_to_watch(watcher);
    update_entropy_file_to_watch(watcher);
}
