use std::{io::Write, ops::{Shl, Shr}, path::Path, env::args, fs::{read, File}, time::{SystemTime, UNIX_EPOCH}};
use sprintf::sprintf;

const TIME_STEP: u128 = 30000000;

fn path_to_string(path: &String) -> Option<String> {
    match read(path) {
        Ok(value) => {
            String::from_utf8(value).ok()
        }
        Err(error) => {
            println!("Error: {}", error);
            println!("Was trying to read: {}", path);
            None
        }
    }
}

fn get_unix_timestamp() -> u128 {
    let current_time: SystemTime = SystemTime::now();
    current_time.duration_since(UNIX_EPOCH).expect("WTF WE GOT BACK IN TIME").as_micros()
}

fn get_hashed_key(path: &String) -> Option<Vec<u8>> {
    path_to_string(&String::from(path)).map(
        |value| value.as_bytes().to_vec()
    )
}

fn hmac_sha1(secret: &[u8], message: &[u8; 8]) -> Vec<u8> {
    let hash_nbr_3: Vec<u8> = sha1_input(&[secret.iter().map(|x| x ^ 0x36).collect(), message.to_vec()].concat()).unwrap();
    let nbr_4: Vec<u8> = [secret.iter().map(|x| x ^ 0x5C).collect(), hash_nbr_3].concat();
    let result: Vec<u8> = sha1_input(&nbr_4).unwrap();
    result
}

fn sha1_input(data: &[u8]) -> Option<Vec<u8>> {
    let mut w: [u32; 80];
    let mut h: [u32; 5] = [0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0];
    let databytes: u32 = data.len() as u32;

    let mut widx: u32;
    let mut didx: u32 = 0;

    let databits: u64 = (databytes) as u64 * 8;
    let loopcount: u32 = (databytes + 8) / 64 + 1;
    let tailbytes: u32 = 64 * loopcount - databytes;
    let mut datatail: [u8; 128] = [0; 128];

    datatail[0] = 0x80;
    datatail[tailbytes as usize - 8] = (databits >> 56 & 0xFF) as u8;
    datatail[tailbytes as usize - 7] = (databits >> 48 & 0xFF) as u8;
    datatail[tailbytes as usize - 6] = (databits >> 40 & 0xFF) as u8;
    datatail[tailbytes as usize - 5] = (databits >> 32 & 0xFF) as u8;
    datatail[tailbytes as usize - 4] = (databits >> 24 & 0xFF) as u8;
    datatail[tailbytes as usize - 3] = (databits >> 16 & 0xFF) as u8;
    datatail[tailbytes as usize - 2] = (databits >> 8 & 0xFF) as u8;
    datatail[tailbytes as usize - 1] = (databits & 0xFF) as u8;
    for _lidx in 0..loopcount {
        w = [0; 80];
        widx = 0;
        while widx <= 15 {
            let mut wcount: i32 = 24;
            while didx < databytes && wcount >= 0 {
                w[widx as usize] += (data[didx as usize] as u32).shl(wcount);
                didx += 1;
                wcount -= 8;
            }
            while wcount >= 0 {
                w[widx as usize] += (datatail[(didx - databytes) as usize] as u32).shl(wcount);
                didx += 1;
                wcount -= 8;
            }
            widx += 1;
        }
        widx = 16;
        while widx <= 31 {
            w[widx as usize] = (w[(widx as usize) - 3] ^ w[(widx as usize) - 8] ^ w[(widx as usize) - 14] ^ w[(widx as usize) - 16]).rotate_left(1);
            widx += 1;
        }
        widx = 32;
        while widx <= 79 {
            w[widx as usize] = (w[(widx as usize) - 6] ^ w[(widx as usize) - 16] ^ w[(widx as usize) - 28] ^ w[(widx as usize) - 32]).rotate_left(2);
            widx += 1;
        }
        let mut a: u32 = h[0];
        let mut b: u32 = h[1];
        let mut c: u32 = h[2];
        let mut d: u32 = h[3];
        let mut e: u32 = h[4];
        let mut f: u32 = 0;
        let mut k: u32 = 0;
        for idx in 0..80 {
            if idx <= 19 {
                f = (b & c) | ((!b) & d);
                k = 0x5A827999;
            } else if (20..=39).contains(&idx) {
                f = b ^ c ^ d;
                k = 0x6ED9EBA1;
            } else if (40..=59).contains(&idx) {
                f = (b & c) | (b & d) | (c & d);
                k = 0x8F1BBCDC;
            } else if (60..=79).contains(&idx) {
                f = b ^ c ^ d;
                k = 0xCA62C1D6;
            }
            let temp: u32 = a.rotate_left(5).wrapping_add(f).wrapping_add(e).wrapping_add(k).wrapping_add(w[idx as usize]);
            e = d;
            d = c;
            c = b.rotate_left(30);
            b = a;
            a = temp;
        }
        h[0] = h[0].wrapping_add(a);
        h[1] = h[1].wrapping_add(b);
        h[2] = h[2].wrapping_add(c);
        h[3] = h[3].wrapping_add(d);
        h[4] = h[4].wrapping_add(e);
    }
    let result = match sprintf!("%08x%08x%08x%08x%08x", h[0],h[1],h[2],h[3],h[4]) {
        Ok(value) => value,
        Err(error) => {
            println!("Error when calling sprintf: {:?}", error);
            return None;
        }
    };
    Some(result.as_bytes().to_vec())
}

fn save_key(hashed_key: &[u8]) -> Option<()> {
    let name_file: &Path = Path::new("ft_otp.key");
    let mut file: File = match File::create(name_file) {
        Ok(value) => value,
        Err(error) => {
            println!("Error when opening file: {}", error);
            return None;
        }
    };
    match file.write(hashed_key) {
        Ok(_len_written) => (),
        Err(error) => {
            println!("Error when writing to file: {}", error);
            return None;
        }
    }
    Some(())
}

fn hash_key(file: &str) -> Option<Vec<u8>> {
    let content_file: String = match path_to_string(&file.to_string()) {
        None => return None,
        Some(x) => x,
    };
    for letter in content_file.chars() {
        let tmp = letter.to_ascii_uppercase();
        if !"0123456789ABCDEF".contains(tmp) {
            println!("Error: non hexadecimal character found: {}", tmp);
            return None;
        }
    }
    if content_file.len() != 64 {
        println!("Need 64 hex char");
        return None;
    }
    sha1_input(&content_file.into_bytes())
}

fn generate_tmp_key(path: &String) -> Option<()> {
    let secret: Vec<u8> = match get_hashed_key(path) {
        None => return None,
        Some(value) => value,
    };
    let current_time: u128 = get_unix_timestamp();
    let mut t: u128 = current_time / TIME_STEP;
    println!("{} seconds till new code  ", (TIME_STEP - (current_time % TIME_STEP)) / 1000000);
    let mut time_bytes: [u8; 8] = [0u8; 8];
    for i in (0..8).rev() {
        time_bytes[i] = (t & 0xFFu128) as u8;
        t = t.shr(8);
    }
    let hmac: Vec<u8> = hmac_sha1(&secret, &time_bytes);
    let offset: usize = (hmac.last().unwrap() & 0x0F) as usize;
    let binary_code: u32 = ((u32::from(hmac[offset]) & 0x7f) << 24)
        | (u32::from(hmac[offset + 1]) << 16)
        | (u32::from(hmac[offset + 2]) << 8)
        | u32::from(hmac[offset + 3]);
    println!("Your code is {}", binary_code % 10u32.pow(8));
    Some(())
}

fn main() {
    let args: Vec<String> = args().collect();
    if args.len() < 3 || args.len() > 5 {
        println!("Usage: ./ft_otp -g key.txt [-k ft_otp.key]");
        return;
    }
    for iter in (1..args.len()).step_by(2) {
        if args[iter] == "-g" {
            let hashed_key: Vec<u8> = match hash_key(&(args[iter + 1])) {
                None => return,
                Some(x) => x,
            };
            println!("Key hashed!");
            match save_key(&hashed_key) {
                None => return,
                Some(_x) => (),
            }
        } else if args[iter] == "-k" {
            generate_tmp_key(&args[iter + 1]);
        } else {
            panic!("Wrong argument\nUsage: ./ft_otp -g key.txt [-k ft_otp.key]");
        }
    }
}