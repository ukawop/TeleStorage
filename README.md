# TeleStorage
Program for using telegram as a free cloud.

Please note that the functionality has been verified only on Ubuntu.

## Installation

To run the program, you can use one of the following methods:

### Method 1: Cloning the Source Code

If you prefer to download the source code, execute the following commands:

```
git clone https://github.com/ukawop/TeleStorage.git
```
Update the package list and install the required dependencies:
```
sudo apt update
sudo apt install cmake g++ libncurses5-dev libsqlite3-dev libssl-dev libboost-system-dev
```
Navigate into TeleStorage and open .env for editing:
```
cd TeleStorage
nano .env
```
Enter your bot token, which you can obtain token from [BotFather](https://t.me/BotFather) and save the file.

Create a build directory and navigate into it:
```
mkdir build
cd build
```
Build the project:
```
cmake ..
make
```
Run the program:
```
./TeleStorage
```


### Method 2: Downloading the Release

1. Go to the [releases page](https://github.com/ukawop/TeleStorage/releases) and download the latest version.
2. Extract the downloaded archive.
3. Enter your bot token in the `.env` file. You can obtain token from [BotFather](https://t.me/BotFather).
4. Run the program from the terminal.

## Usage

After starting the program, by pressing 'A', you will be able to enter the path to the file you want to upload to the Telegram server.

![image](https://github.com/user-attachments/assets/b10b8fd7-beb0-4117-9ffc-b609824a62db)

The file is sent in parts of 19 MB to bypass the maximum file size limit.

![image](https://github.com/user-attachments/assets/1836e87f-c63f-4ca5-a3cf-7633ee1b5a72)

After you upload your documents, you can download them back. They will be stored in the directory TeleStorage/documents.
