# Image Steganography using LSB Encoding and Decoding

##  Project Overview

This project implements **Image Steganography** using the **Least Significant Bit (LSB)** technique in Embedded C.
The main objective of the project is to securely hide a secret text message inside a BMP image file without noticeably changing the image quality.

The project supports both:

* **Encoding** → Hiding secret data inside an image
* **Decoding** → Extracting hidden data from the encoded image

---

#  Features

* Encode secret text inside a BMP image
* Decode hidden text from encoded image
* Uses LSB (Least Significant Bit) technique
* Command-line based execution
* Efficient bitwise manipulation
* Simple and lightweight implementation
* Secure text hiding mechanism

---

#  Technologies Used

* Embedded C
* File Handling
* Bitwise Operations
* Pointers
* Functions
* Structures
* Command Line Arguments

---

#  Project Structure

```bash
├── encode.c
├── decode.c
├── common.c
├── encode.h
├── decode.h
├── common.h
├── types.h
├── test_encode.bmp
├── secret.txt
├── stego.bmp
├── output.txt
└── README.md
```

---

#  Working Principle

## Encoding Process

1. Read the source BMP image
2. Read the secret text file
3. Encode:

   * Magic string
   * Secret file extension
   * Secret file size
   * Secret data
4. Store secret bits into image byte LSBs
5. Generate encoded output image

---

## Decoding Process

1. Read encoded BMP image
2. Decode magic string
3. Extract file extension and file size
4. Decode hidden data bit by bit
5. Store recovered data into output file

---

#  LSB Encoding Concept

LSB (Least Significant Bit) encoding hides data inside the least significant bits of image bytes.

### Example

Original byte:

```text
10110110
```

Secret bit:

```text
1
```

Modified byte:

```text
10110111
```

Only the last bit changes, so the image remains visually unchanged.

---

#  Compilation

Use GCC compiler to build the project.

```bash
gcc *.c -o stego
```

---

#  Execution

## Encoding

```bash
./stego -e test_encode.bmp secret.txt stego.bmp
```

## Decoding

```bash
./stego -d stego.bmp output.txt
```

---

#  Sample Output

## Encoding

```text
INFO: Encoding Started
INFO: Secret file encoded successfully
INFO: Output image generated
```

## Decoding

```text
INFO: Decoding Started
INFO: Secret data extracted successfully
INFO: Output file created
```

---

#  Applications

* Secure Communication
* Digital Watermarking
* Data Hiding
* Cybersecurity Projects
* Information Protection

---

#  Future Enhancements

* Support PNG and JPEG formats
* Password-protected steganography
* Audio and video steganography
* GUI-based implementation
* Encryption before encoding

---

#  Learning Outcomes

Through this project, I learned:

* Bitwise manipulation techniques
* BMP image file handling
* File I/O operations in C
* Command-line argument handling
* Memory management using pointers
* Real-world implementation of data hiding

---

#  Author

**Bhuvaneshwaran**

