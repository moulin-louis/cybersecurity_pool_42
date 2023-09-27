use pcap::Device;

fn main() {
    let devices = match Device::list() {
        Ok(val) => val,
        Err(err) => {
            println!("Error listing devices: {}", err);
            return ;
        }
    };
    for device in devices {
        println!("{:?}", device);
    }
}
