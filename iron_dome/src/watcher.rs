use crate::read_file;
use crate::entropy_check::handle_entropy_change;
use std::{collections::HashMap, env, path::PathBuf};
use entropy::shannon_entropy;
use sysinfo::{System, SystemExt};

#[derive(Debug)]
pub struct Watcher {
    pub path_to_watch: Vec<String>,
    pub files_to_watch: Vec<PathBuf>,
    pub file_watched: HashMap<PathBuf, f32>,
    pub system_info: System,
}

impl Default for Watcher {
    fn default() -> Self {
        Watcher {
            file_watched: HashMap::new(),
            files_to_watch: Vec::new(),
            system_info: System::new(),
            path_to_watch: {
                let mut tmp: Vec<String> = env::args().collect();
                tmp.remove(0);
                tmp
            },
        }
    }
}

impl Watcher {
    pub fn update_entropy(&mut self, val: &PathBuf) {
        let content: Vec<u8> = match read_file(val.clone()) {
            Ok(result) => result,
            Err(err) => {
                match err {
                    1 => return,
                    2 => {
                        //removing val to file_to_watch
                        self.files_to_watch.remove(
                            self.files_to_watch
                                .iter()
                                .position(|r| r == val)
                                .unwrap()
                        );
                        return; },
                    _ => return,
                };
            },
        };
        let new_entropy: f32 = shannon_entropy(content);
        if self.file_watched.contains_key(val) {
            let ref_value: &mut f32 = self.file_watched.get_mut(val).unwrap();
            handle_entropy_change(new_entropy - *ref_value, val);
            *ref_value = new_entropy;
        } else {
            self.file_watched.insert(val.clone(), new_entropy);
        }
    }
}