CREATE DATABASE IF NOT EXISTS digitalStorage;

USE digitalStorage;

CREATE TABLE IF NOT EXISTS digital_id (
  id INT NOT NULL UNIQUE,
  name VARCHAR(255)
);
