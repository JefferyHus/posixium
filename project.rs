use std::fs::{File, OpenOptions};
use std::io::{self, Write};
use std::path::Path;

// Define the size of arguments, command
const MAX_INPUT_SIZE: usize = 1024;

// Sanitize string
fn sanitize_string(input: &str) -> String {
    let whitelist = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-.";
    input.chars()
        .filter(|c| whitelist.contains(*c))
        .collect()
}

// Command trait
trait Command {
    fn execute(&self, args: &str);
}

// Touch command
struct Touch;

impl Command for Touch {
    fn execute(&self, args: &str) {
        let mut parts = args.split_whitespace();
        let filename = parts.next().map(sanitize_string);
        let option = parts.next().map(sanitize_string);
        let value = parts.next().map(sanitize_string);

        if let Some(filename) = filename {
            let path = Path::new(&filename);

            let mut file = if path.exists() {
                OpenOptions::new().write(true).append(true).open(path).unwrap()
            } else {
                File::create(path).unwrap()
            };

            if let Some(option) = option {
                if option == "--content" {
                    if let Some(value) = value {
                        writeln!(file, "{}", value).expect("Unable to write to file");
                    }
                }
            }
        }
    }
}

// Command dispatcher
fn execute_command(name: &str, args: &str) {
    match name {
        "touch" => Touch.execute(args),
        _ => eprintln!("Unknown command"),
    }
}

fn main() {
    let mut input = String::new();

    loop {
        println!("> ");
        input.clear();
        if io::stdin().read_line(&mut input).is_err() {
            break; // Exit on error
        }

        let input = input.trim();
        if input == "exit" {
            break;
        }

        let mut parts = input.splitn(2, ' ');
        let command_name = parts.next().unwrap();
        let args = parts.next().unwrap_or("");

        execute_command(command_name, args);
    }
}
