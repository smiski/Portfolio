/*
* Table: Clients
* Stores client information including name and archival status.
* Clients are soft-deleted by setting the is_archived flag to TRUE.
*/
CREATE TABLE Clients (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    is_archived BOOLEAN NOT NULL DEFAULT FALSE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);

/*
* Table: PhoneNumbers
* Stores phone numbers associated with clients.
* Each phone number belongs to a single client, while a client can have multiple phone numbers.
* Each phone number references a client via a foreign key.
* On deletion of a client, associated phone numbers are also deleted (CASCADE).
*/
CREATE TABLE PhoneNumbers (
    id INT AUTO_INCREMENT PRIMARY KEY,
    client_id INT NOT NULL,
    phone_number VARCHAR(50) NOT NULL,
    CONSTRAINT fk_phone_client
        FOREIGN KEY (client_id) 
        REFERENCES Clients(id)
        ON DELETE CASCADE
);