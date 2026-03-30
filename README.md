# RLE Stack-Based Compressor

### What is RLE?
Run-Length Encoding (RLE) is a simple form of lossless data compression. It works by replacing sequences of identical data (called "runs") with the data value and the number of times it repeats consecutively.

**Example:**
* **Original Data:** AAAAAABBBCCCCD
* **Compressed Data:** 6A3B4C1D

The algorithm is extremely effective on files containing many repetitions, but it can be inefficient (potentially increasing the file size) if the data is highly varied.

---

### Project Objectives
This implementation was developed to study the behavior of the algorithm under strict memory constraints:

* **Memory Management:** The program does not use dynamic allocation (malloc, free). All data loading and transformation occur within the process stack.
* **Buffer Limit:** Data handling is restricted to a `PAGE_SIZE` constant fixed at 4096 bytes.
