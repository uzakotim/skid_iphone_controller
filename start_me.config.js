module.exports = {
    apps: [
        {
            name: "wifi_controller",
            script: "./build/run_me",
            exec_interpreter: "none",
            exec_mode: "fork_mode",
        },
    ],
};
