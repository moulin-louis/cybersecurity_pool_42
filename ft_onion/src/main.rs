use std::env::args;
use std::fs::read;
use std::process::exit;

#[derive(Debug)]
struct Context {
    int_has: [u32; 5],
    len_low: u32,
    len_high: u32,
    mesg_block_index: i16,
    msgb_block: [u8; 64],
    corupted: i32,
}

impl Default for Context {
    fn default() -> Self {
        Context {
            int_has: [0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0],
            len_low: 0,
            len_high: 0,
            mesg_block_index: 0,
            msgb_block: [0; 64],
            corupted: 0,
        }
    }
}

fn path_to_string(path: &String) -> String {
    let result: String;
    match read(path) {
        Ok(value) => result = String::from_utf8(value).unwrap(),
        Err(error) => {
            println!("Error: {}", error);
            println!("Was trying to read: {}", path);
            exit(1);
        }
    }
    result
}

#[allow(arithmetic_overflow)]
fn process_msg_block(context: &mut Context) {
    let k: [u32; 4] = [ 0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6 ];
    let mut w: [u8; 80] = [0; 80];
    let mut a: u32 = context.int_has[0];
    let mut b: u32 = context.int_has[1];
    let mut c: u32 = context.int_has[2];
    let mut d: u32 = context.int_has[3];
    let mut e: u32 = context.int_has[4];
    let mut iter: usize = 0;
    while iter < 16 {
        w[iter] = context.msgb_block[iter * 4] << 24;
        w[iter] |= context.msgb_block[iter * 4 + 1] << 16;
        w[iter] |= context.msgb_block[iter * 4 + 2] << 8;
        w[iter] |= context.msgb_block[iter * 4 + 3];
        iter += 1;
    }
    iter = 16;
    while iter < 80 {
        let bits: u8 = w[iter - 3] ^ w[iter -8] ^ w[iter - 14] ^ w[iter - 16];
        w[iter] = (1 << (bits)) | (1 >> (32-(bits)));
        iter += 1;
    }
    iter = 0;
    while iter < 20 {
        let temp = ((5) << (a)) | ((5) >> (32-(a))) + ((b & c) | ((!b) & d) + e + w[iter] as u32 + k[0]);
        e = d;
        d = c;
        c = ((30) << (b)) | ((30) >> (32-(b)));
        b = a;
        a = temp;
        iter += 1;
    }
    iter = 20;
    while iter < 40 {
        let temp = ((5) << (a)) | ((5) >> (32-(a))) + (b ^ c ^ d) + e + w[iter] as u32+ k[1];
        e = d;
        d = c;
        c = ((30) << (b)) | ((30) >> (32-(b)));
        b = a;
        a = temp;
        iter += 1;
    }
    iter = 40;
    while iter < 60 {
        let temp = ((5) << (a)) | ((5) >> (32-(a))) + ((b & c) | (b & d) | (c & d) + e + w[iter] as u32 + k[2]);
        e = d;
        d = c;
        c =((30) << (b)) | ((30) >> (32-(b)));
        b = a;
        a = temp;
        iter +=1;
    }
    iter = 60;
    while iter < 80 {
        let temp = ((5) << (a)) | ((5) >> (32-(a))) + (b ^ c ^ d) + e + w[iter] as u32 + k[3];
        e = d;
        d = c;
        c =((30) << (b)) | ((30) >> (32-(b)));
        b = a;
        a = temp;
        iter += 1;
    }
    context.int_has[0] += a;
    context.int_has[1] += b;
    context.int_has[2] += c;
    context.int_has[3] += d;
    context.int_has[4] += e;
    context.mesg_block_index = 0;
}

fn pad_msg(context: &mut Context) {
    if context.mesg_block_index > 55 {
        context.msgb_block[context.mesg_block_index as usize] = 0x80;
        context.mesg_block_index += 1;
        while context.mesg_block_index < 64 {
            context.msgb_block[context.mesg_block_index as usize] = 0;
            context.mesg_block_index += 1;
        }
        process_msg_block(context);
        while context.mesg_block_index < 56 {
            context.msgb_block[context.mesg_block_index as usize] = 0;
            context.mesg_block_index += 1;
        }
    } else {
        context.msgb_block[context.mesg_block_index as usize] = 0x80;
        context.mesg_block_index += 1;
        while context.mesg_block_index < 56 {
            context.msgb_block[context.mesg_block_index as usize] = 0;
            context.mesg_block_index += 1;
        }
    }
    context.msgb_block[56] = (context.len_high >> 24) as u8;
    context.msgb_block[57] = (context.len_high >> 16) as u8;
    context.msgb_block[58] = (context.len_high >> 8) as u8;
    context.msgb_block[59] = context.len_high as u8;
    context.msgb_block[60] = (context.len_low >> 24) as u8;
    context.msgb_block[61] = (context.len_low >> 16) as u8;
    context.msgb_block[62] = (context.len_low >> 8) as u8;
    context.msgb_block[63] = context.len_low as u8;
    process_msg_block(context);
}

fn sha1_input(keys: &String, context: &mut Context) {
    if context.corupted == 1 {
        panic!("Context corrupted!");
    }
    let mut len = 512;
    let mut iter: usize = 0;
    while len != 0 && context.corupted == 0 && iter != keys.len() {
        context.msgb_block[context.mesg_block_index as usize] = keys.chars().nth(iter).unwrap() as u8 & 0xFF;
        context.mesg_block_index += 1;
        context.len_low += 8;
        if context.len_low == 0 {
            context.len_high += 1;
            if context.len_high == 0 {
                context.corupted = 1;
            }
        }
        if context.mesg_block_index == 64 {
            process_msg_block(context);
        }
        iter +=1;
        len -=1;
    }
}

fn hash_key(file: &String) {
    let content_file = path_to_string(file);
    if content_file.len() != 64 {
        println!("Need 64 hex char");
        exit(1);
    }
    let mut context: Context = Context::default();
    sha1_input(&content_file, &mut context);
    pad_msg(&mut context);
    for i in 1..64 {
        context.msgb_block[i] = 0;
    }
    context.len_low = 0;
    context.len_high = 0;
    let mut result: Vec<u8> = Vec::new();
    for i in 0..20 {
        result.push((context.int_has[i >> 2] >> 8 * (3-(i & 0x03))) as u8);
    }
    for letter in result {
        print!("{:X}", letter);
    }
}
fn main() {
    let args: Vec<String> = args().collect();
    if args.len() < 3 {
        println!("Usage: ./ft_otp -g key.txt [-k ft_otp.key]");
        return ;
    }
    for iter in (1..args.len()).step_by(2) {
        if args[iter] == "-g" {
            hash_key(&(args[iter + 1]));
        } else if args[iter] == "-k" {
            todo!("Handling k option");
        } else {
            panic!("Wrong argument\nUsage: ./ft_otp -g key.txt [-k ft_otp.key]");
        }
    }

}
