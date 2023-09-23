use std::{
    env,
    fs,
    collections::HashMap,
    fs::{DirEntry, ReadDir},
    path::PathBuf,
};
use entropy::shannon_entropy;

macro_rules! dprintln {
    ($($arg:tt)*) => {
        #[cfg(debug_assertions)]
        {
            println!($($arg)*);
        }
    };
}

#[derive(Debug)]
struct Watcher {
    path_to_watch: Vec<String>,
    file_watched: HashMap<PathBuf, f32>,
}

impl Default for Watcher {
    fn default() -> Self {
        Watcher {
            file_watched: HashMap::default(),
            path_to_watch: {
                let mut tmp: Vec<String> = env::args().collect();
                tmp.remove(0);
                tmp
            },
        }
    }
}

impl Watcher {
    fn is_file_watched(&self, val: &DirEntry) -> bool {
        self.file_watched.contains_key(&val.path())
    }

    fn update_entropy(&mut self, val: &DirEntry) {
        let new_entropy = shannon_entropy(val.path().into_os_string().into_string().unwrap().as_bytes());
        dprintln!("New entrop for {:?} = {}", val.path(), new_entropy);
        let ref_value = self.file_watched.get_mut(&val.path()).unwrap();
        *ref_value = new_entropy;
    }
    fn add_file(&mut self, val: &DirEntry) {
        let entropy = shannon_entropy(val.path().into_os_string().into_string().unwrap().as_bytes());
        dprintln!("Adding file {:?} to watch with entropy of {}", val.path(), entropy);
        self.file_watched.insert(val.path(), entropy);
    }
}

fn main() {
    let mut watcher = Watcher::default();
    if watcher.path_to_watch.is_empty() {
        println!("No path provided, default to $HOME");
        watcher.path_to_watch.push(env::var("HOME").expect("Cant find HOME env var"));
    }

    for index in 0..watcher.path_to_watch.len() {
        let list_files: ReadDir = match fs::read_dir(&watcher.path_to_watch[index]) {
            Ok(value) => value,
            Err(err) => {
                eprintln!("Error reading files of {}: {err}", watcher.path_to_watch[index]);
                continue;
            },
        };
        for file in list_files {
          match file {
              Err(err) => {
                  eprintln!("Error reading file: {}", err);
                  continue;
              },
              Ok(val) => {
                  if watcher.is_file_watched(&val) {
                      watcher.update_entropy(&val);
                  } else {
                      watcher.add_file(&val);
                  }
              },
          };
        }
    }
}