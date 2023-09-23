use tokio::fs::File;
use tokio::io::{self, AsyncReadExt};

#[tokio::main]
async fn main() -> io::Result<()> {
    let mut file = File::open("test.txt").await?;
    let mut buffer = [0; 10];
    let read = file.read(&mut buffer).await?;
    println!("Read {:?}", &buffer[..read]);
    Ok(())

}