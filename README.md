# lab-systems-test

## Description

The program represents a tool for analyzing textual content written in the English language. It is designed to uncover the structure and frequency of occurrence of words in a text file, providing valuable statistical insights.

At the core of the program's functionality lies a mechanism for counting each unique word and determining the number of its occurrences in the text. The results are recorded in the output file, sorted based on two criteria: the primary criterion is the frequency of occurrences, and the secondary criterion is alphabetical order.

## Build & Run


Run headless (app without GUI) app

```batch
> cd headless
> cmake .
> cmake --build .
```
Move file within executable directory and run it through terminal
```batch
> cd Debug
> lab-systems-test.exe
```

To build and run, simply open your Qt Creator 4.11.1 & Desktop Qt 5.14.2 MinGW 32-bit (as per my setup).