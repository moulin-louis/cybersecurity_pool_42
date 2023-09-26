use std::{fs::File, collections::HashMap, io::{Read, Seek, SeekFrom}};

const THRESHOLD_READ: u64 = 10000000; //10MB

fn check_abuse(disk: String, new_value: u64, old_value: u64) {
    if new_value - old_value > THRESHOLD_READ {
        println!("Potential disk abuse detected for [{}]", disk);
    }
}

pub fn detect_disk_read_abuse(watcher: &mut HashMap<String, u64>) {
    let mut fd: File = File::open("/proc/diskstats").unwrap();
    let mut curr: HashMap < String, u64 > = HashMap::new();
    let mut io_data: String = String::new();
    fd.read_to_string(&mut io_data).unwrap();
    for line in io_data.lines() {
        let fields: Vec<&str> = line.split_whitespace().collect();
        if fields[2].starts_with("loop") {
            continue;
        }
        let ds: u64 = fields[5].parse::<u64>().unwrap();
        if watcher.contains_key(fields[2]) {
            check_abuse(fields[2].to_string(), ds,*watcher.get(fields[2]).unwrap());
        }
        if watcher.contains_key(fields[2]) || watcher.is_empty() {
            curr.insert(fields[2].to_owned(), ds);
        }
    }
    fd.seek(SeekFrom::Start(0)).unwrap();
    *watcher = curr;
    // for (name, read) in &watcher.disk_read {
    //     println!("disk {}, {} MB", name, (read * 512) / 2048);
    // }
}