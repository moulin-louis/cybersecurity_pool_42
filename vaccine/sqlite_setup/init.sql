CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    email TEXT NOT NULL, -- assuming emails should be unique
    password TEXT NOT NULL
);

-- Insert data into users table
INSERT INTO users (name, email, password) VALUES
('Alice', 'alice@email.com', 'password123'),
('Bob', 'bob@email.com', 'password123'),
('Charlie', 'charlie@email.com', 'password123');

CREATE TABLE IF NOT EXISTS users3 (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    email TEXT NOT NULL, -- assuming emails should be unique
    password TEXT NOT NULL
);

-- Insert data into users table
INSERT INTO users (name, email, password) VALUES
('Afsdfsd', 'alice@email.com', 'passfsdfsrd123'),
('fdsfBob', 'berwerob@email.com', 'password123'),
('fsdf', 'fdfemail.com', 'khjkh');