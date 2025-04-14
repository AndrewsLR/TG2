import os
import subprocess
import sys

# File name containing the data
input_file = "44-6.genlib"
# Name of the executable
executable = "./imp_pandr.exe"

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
            # Just pass the argument directly without additional quotes
            print(f"Launching {executable_path} with argument: {arg}")
            subprocess.run([executable_path, arg], check=True)
            #input("Press Enter to continue...")  # Wait for user input before proceeding
        except FileNotFoundError:
            print(f"Error: Executable {executable_path} not found.")
        except subprocess.CalledProcessError as e:
            print(f"Error: {e}")


def main():
    if not os.path.isfile(executable):
        print(f"Error: Executable {executable} not found in the current directory.")
        return

    boolean_expressions = extract_boolean_expressions(input_file)
    if not boolean_expressions:
        print("No boolean expressions found.")
        return

    try:
        launch_executable(executable, boolean_expressions)
    except subprocess.CalledProcessError as e:
        print(f"\nExecution failed: {e}")
        print("Stopping script due to crash.")
        sys.exit()
    except Exception as e:
        print(f"\nUnexpected error: {e}")
        print("Stopping script.")
        sys.exit()
if __name__ == "__main__":
    main()
