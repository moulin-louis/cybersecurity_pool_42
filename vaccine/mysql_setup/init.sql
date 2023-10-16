# noinspection SqlNoDataSourceInspectionForFile

CREATE DATABASE IF NOT EXISTS prod;

USE prod;

CREATE TABLE users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    email VARCHAR(255) NOT NULL,
    password VARCHAR(255) NOT NULL
);

INSERT INTO users (name, email, password) VALUES
('Alice', 'alice@email.com', 'password123'),
('Bob', 'bob@email.com', 'password123'),
('Charlie', 'charlie@email.com', 'password123');

CREATE TABLE autre_truc (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    email VARCHAR(255) NOT NULL,
    password VARCHAR(255) NOT NULL
);

INSERT INTO autre_truc (name, email, password) VALUES
('Alicefdsfsd', 'ali42ce@email.com', '4242'),
('Bfsdfsdob', 'bob@emaiddl.com', 'ifdsf'),
('dffsd', 'charlie@efdsmail.com', 'password123');