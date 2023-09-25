use crate::watcher::Watcher;
use std::{path::PathBuf, fs::{read_dir, ReadDir}};

pub fn handle_entropy_change( diff_entro: f32, val: &PathBuf) {
    if diff_entro.abs() > 1.0 {
        println!("Potential encryption of file: {:?}", val);
    }
}

fn updating_files_to_watch(watcher: &mut Watcher) {
    for index in 0..watcher.path_to_watch.len() {
        let list_files: ReadDir = match read_dir(&watcher.path_to_watch[index]) {
            Ok(value) => value,
            Err(err) => {
                eprintln!("Error reading files of {}: {err}", watcher.path_to_watch[index]);
                continue;
            },
        };
        for path in list_files {
            match path {
                Err(err) => {
                    eprintln!("Error read_dir: {}", err);
                },
                Ok(val) => {
                    if val.file_type().unwrap().is_dir() {
                        continue;
                    }
                    if !watcher.files_to_watch.contains(&val.path()) {
                        watcher.files_to_watch.push(val.path());
                    }
                }
            }
        }
    }
}

fn update_entropy_file_to_watch(watcher: &mut Watcher) {
    for index in 0..watcher.files_to_watch.len() {
        if watcher.update_entropy(&watcher.files_to_watch[index].clone()).is_none() {
            eprintln!("files_to_watch change, aborting current loop");
            break;
        }
    }
}

pub fn detect_entropy_change(watcher: &mut Watcher) {
    updating_files_to_watch(watcher);
    update_entropy_file_to_watch(watcher);
}