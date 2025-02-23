#pragma once
#include <fstream>
#include <string>

pid_t runCommand(const std::string &command, const std::string &logfile) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process: Redirect stdout and stderr to logfile
        freopen(logfile.c_str(), "w", stdout);
        freopen(logfile.c_str(), "w", stderr);

        // Execute the command
        execl("/bin/sh", "sh", "-c", command.c_str(), (char *)NULL);
        // If execl returns, it must have failed
        perror("execl");
        exit(EXIT_FAILURE);
    }
    // Parent process: Return the child's PID
    return pid;
}

void kill_process(int pid) {
    if (kill(pid, SIGKILL) == 0) {
        std::cout << "Command with PID " << pid << " terminated." << std::endl;
    } else {
        perror("kill");
    }

    std::string command = "pgrep -P " + std::to_string(pid);
    FILE *fp = popen(command.c_str(), "r");
    if (fp) {
        char pid_buf[10];
        while (fgets(pid_buf, sizeof(pid_buf), fp)) {
            int child_pid = std::stoi(pid_buf);
            kill_process(child_pid);
        }
        pclose(fp);
    }
}

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    std::string content;

    if (file.is_open()) {
        // Read the entire file content into the string
        content.assign((std::istreambuf_iterator<char>(file)),
                       (std::istreambuf_iterator<char>()));

        file.close();
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }

    return content;
}

std::string stripTimestampField(const std::string& expected) {
    // Find the position of the last space in the string
    size_t lastSpacePos = expected.rfind(' ');
    if (lastSpacePos != std::string::npos) {
        // Return the substring up to (but not including) the last space
        return expected.substr(0, lastSpacePos) + "\n";
    }
    return expected; // Return the original string if no space found
}
