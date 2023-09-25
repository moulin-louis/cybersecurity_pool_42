use crate::watcher::Watcher;
use std::fs::{read_dir, ReadDir};
use std::path::PathBuf;

fn updating_files_to_watch(watcher: &mut Watcher) {
    let mut to_delete: Vec<String> = Vec::new();
    //iter over all path to watch
    for index in 0..watcher.path_to_watch.len() {
        //list all files and dir into this path
        let list_files: ReadDir = match read_dir(&watcher.path_to_watch[index]) {
            Ok(value) => value,
            Err(err) => {
                eprintln!("Error reading files of {}: {err}", watcher.path_to_watch[index]);
                to_delete.push(watcher.path_to_watch[index].clone());
                continue;
            },
        };
        //add every file (not dir) into file_watched
        for path in list_files {
            match path {
                Err(err) => {
                    eprintln!("Error read path: {}", err);
                },
                Ok(val) => {
                    if val.file_type().unwrap().is_dir() {
                        continue;
                    }
                    let tmp = val.path();
                    watcher.file_watched.entry(tmp).or_insert(0.0);
                }
            }
        }
    }
    //delete every dir unavailable from path_to_watch
    for path in to_delete {
        let pos: usize = watcher.path_to_watch.iter().position(|x| x == &path).unwrap();
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