use std::{
    collections::HashMap,
    fs::File,
    io::{Read, Seek, SeekFrom},
    sync::{atomic::AtomicBool, Arc, Mutex, MutexGuard},
};

const THRESHOLD_READ: u64 = 2000000; //2MB

pub fn detect_disk_read_abuse(
    watcher: &mut HashMap<String, u64>,
    flag_arc: &mut Arc<Mutex<[AtomicBool; 3]>>,
) {
    let mut found_disk_sus: bool = false;
    let mut fd: File = File::open("/proc/diskstats").unwrap();
    let mut curr: HashMap<String, u64> = HashMap::new();
    let mut io_data: String = String::new();
    fd.read_to_string(&mut io_data).unwrap();
    for line in io_data.lines() {
        let fields: Vec<&str> = line.split_whitespace().collect();
        if fields[2].starts_with("loop") || fields[2].starts_with("ram") {
            continue;
        }
        let ds: u64 = fields[5].parse::<u64>().unwrap();
        if watcher.contains_key(fields[2]) {
            let read_since: u64 = ds - *watcher.get(fields[2]).unwrap();
            println!("read_since for disk {} = {}", fields[2], read_since);
            if read_since > THRESHOLD_READ {
                found_disk_sus = true;
                //flaging disk_flag for all thread
                let mut flags: MutexGuard<'_, [AtomicBool; 3]> = flag_arc.lock().unwrap();
                flags[1] = AtomicBool::new(true);
                println!("Potential disk abuse detected for [{}]", fields[2]);
            }
        }
        if watcher.contains_key(fields[2]) || watcher.is_empty() {
            curr.insert(fields[2].to_owned(), ds);
        }
    }
    fd.seek(SeekFrom::Start(0)).unwrap();
    *watcher = curr;
    if !found_disk_sus {
        let mut flags: MutexGuard<'_, [AtomicBool; 3]> = flag_arc.lock().unwrap();
        flags[1] = AtomicBool::new(false);
    }
    // for (disk, read) in watcher {
    //     println!("{}: {}/{} sector/bytes read", disk, read, *read * 512);
    // }
}
