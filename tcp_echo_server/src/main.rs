use std::error::Error;
use std::mem;
use std::net::TcpListener;

fn main() -> Result<(), Box<Error>> {
    let listener = TcpListener::bind("127.0.0.1:787898")?;

    let csize = mem::size_of::<TcpListener>();
    let rsize = mem::size_of_val(&listener);
    println!("CSize:{}. RSize:{}", csize, rsize);

    /*
    for stream in listener.incoming() {
        let stream = stream.unwrap();

        println!("Connection established!");
    }
    */
    Ok(())
}
