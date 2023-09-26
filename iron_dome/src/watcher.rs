use std::{collections::HashMap, env, path::PathBuf, fs::File, io::Read, path::Path};
use entropy::shannon_entropy;

const BUFF_SIZE: usize = 1000000;

pub struct Watcher {
    pub path_to_watch: Vec<String>,
    pub file_watched: HashMap<PathBuf, f32>,
}

impl Default for Watcher {
    fn default() -> Self {
        Watcher {
            file_watched: HashMap::new(),
            path_to_watch: {
                let mut tmp: Vec<String> = env::args().collect();
                tmp.remove(0);
                tmp
            },
        }
    }
}

fn get_entropy(path: &Path) -> Option<f32> {
    let mut result: f32 = 0.0;
    let mut f: File = match File::open(path) {
        Ok(val) => val,
        Err(err) => {
            println!("Error opening file: {}", err);
            return None;
        }
    };
    let mut nbr_loop: i32 = 0;
    loop {
        let mut buff: [u8; BUFF_SIZE] = [0; BUFF_SIZE];
        let byte_read: usize = match f.read(&mut buff) {
            Ok(val) => val,
            Err(err) => {
                println!("Error reading file: {}", err);
                return None;
            },
        };
        if byte_read == 0{
            break;
        }
        result += shannon_entropy(&buff[0..byte_read]);
        nbr_loop += 1;
    }
    result /= nbr_loop as f32;
    Some(result)
}

fn handle_entropy_change(old_entro: f32, new_entro: f32, val: &PathBuf) {
    if old_entro != 0.0 && (new_entro - old_entro).abs() > 1.0 {
        println!("Potential encryption of file: {:?}", val);
    }
}

impl Watcher {
    pub fn update_entropy(&mut self, val: &PathBuf) -> Option<()> {
        let new_entropy: f32 = match get_entropy(val) {
            Some(val) => val,
            None => {
                self.file_watched.remove(val);
                return None;
            }
        };
        let ref_value: &mut f32 = self.file_watched.get_mut(val).unwrap();
        handle_entropy_change(*ref_value,new_entropy, val);
        *ref_value = new_entropy;
        Some(())
    }
}