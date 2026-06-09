## Blockchain Attendance System

App that records attendance on a simple blockchain with SHA-256 hashes and ECDSA signatures.

## Required Libraries and Dependencies

- OpenSSL development libraries (`libssl`, `libcrypto`)
- C compiler (e.g., `gcc`)

## Compilation Instructions

```sh
make
```

## How to Build and Run the Application

## Build

Requires OpenSSL (`libssl`, `libcrypto`).

```sh
make
```

## Run

```sh
make run 
```

## Usage

Menu options: mark attendance, view records, validate blockchain, and tamper demo.

## Data Files

- `students.txt`: `student_id,full_name,course_code` per line
