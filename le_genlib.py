import os
import subprocess

# File name containing the data
input_file = "44-6.genlib"
# Name of the executable
executable = "imp_pandr.exe"

def extract_boolean_expressions(file_path):
    """
    Extract boolean expressions from lines starting with 'GATE'.
    Extract only the boolean expression after 'O=' and remove leading '!'.
    """
    boolean_expressions = []
    try:
        with open(file_path, "r") as file:
            for line in file:
                if line.startswith("GATE"):
                    # Extract the part after 'O='
                    o_index = line.find("O=")
                    if o_index != -1:
                        expression = line[o_index + 2:].strip().strip(';')
                        # Remove leading '!' if present
                        if expression.startswith("!"):
                            expression = expression[1:]
                        boolean_expressions.append(expression)
    except FileNotFoundError:
        print(f"Error: File {file_path} not found.")
    return boolean_expressions

def launch_executable(executable_path, arguments):
    """
    Launch the executable with the given arguments.
    """
    for arg in arguments:
        try:
            # Properly format the argument with one set of quotes
            formatted_arg = f'"{arg}"'
            print(f"Launching {executable_path} with argument: {formatted_arg}")
            subprocess.run([executable_path, formatted_arg], check=True)
            input("Press Enter to continue...")  # Wait for user input before proceeding
        except FileNotFoundError:
            print(f"Error: Executable {executable_path} not found.")
        except subprocess.CalledProcessError as e:
            print(f"Error: {e}")

def main():
    # Ensure the executable exists
    if not os.path.isfile(executable):
        print(f"Error: Executable {executable} not found in the current directory.")
        return

    # Extract boolean expressions from the file
    boolean_expressions = extract_boolean_expressions(input_file)
    if not boolean_expressions:
        print("No boolean expressions found.")
        return

    # Launch the executable with each boolean expression as an argument
    launch_executable(executable, boolean_expressions)

if __name__ == "__main__":
    main()