use pcap::{Device, Address, };
use std::net::IpAddr;
use std::str::FromStr;

#[derive(Debug)]
struct Args {
    ip_src: IpAddr,
    mac_src: String,
    ip_target: IpAddr,
    mac_target: String,
}

fn usage() {
    println!("./inquisitor <IP-src> <MAC-src> <IP-target> <MAC-target> ");
}

fn parse_args(args: &[String]) -> Option<Args> {
    Some (Args{
        ip_src: IpAddr::from_str(&args[1].clone()).unwrap(),
        mac_src: args[2].clone(),
        ip_target: IpAddr::from_str(&args[3].clone()).unwrap(),
        mac_target: args[4].clone(),
    })
}

fn main() {
    let argv: Vec<String> = std::env::args().collect();
    if argv.len() != 5 {
        eprintln!("Wrong number of argument");
        usage();
        return ;
    }
    let args:Args = parse_args(&argv).unwrap();
    println!("args = {:?}", args);

    let devices: Vec<Device> = match Device::list() {
        Ok(val) => val,
        Err(err) => {
            eprintln!("Error listing devices: {}", err);
            return ;
        }
    };
    for device in devices {
        println!("{:?}", device.name);
    }
}
