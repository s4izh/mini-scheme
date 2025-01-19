import sys
from typing import List
import argparse

class Transition():
    def __init__(self, state_prev, event, state_next):
        self.state_prev = state_prev
        self.event = event
        self.state_next = state_next

    def __repr__(self):
        return f"{self.state_prev}, {self.event}, {self.state_next}"

    def to_c_func_call(self):
        ev_str = ""
        for i, ev in enumerate(self.event):
            ev_str += ev
            if i < len(self.event) - 1:
                ev_str += " "

        if ev_str == "ε":
            return None

        return f"nfa_add_transition(nfa, {self.state_prev}, {ev_str}, {self.state_next});"

    def add_prefix(self, prefix):
        self.state_prev = prefix + self.state_prev
        self.state_next = prefix + self.state_next

def extract_transitions(plantuml_input) -> List[Transition]:
    transitions = []

    for line in plantuml_input.split("\n"):
        splits = line.split(" ")
        if len(splits) >= 5:
            splits = line.split(" ")
            splits.pop(1)
            splits.pop(2)
            if splits[1] != "[*]":
                transitions.append(Transition(splits[0], splits[2:], splits[1]))

    return transitions

def main():
    parser = argparse.ArgumentParser(description="Helper script to generate NFA transitions")
    parser.add_argument("--state-prefix", type=str, help="Prefix to add to state name")
    parser.add_argument("--event-prefix", type=str, help="Prefix to add to event name")
    parser.add_argument("-i", "--input", type=str, help="PlantUML source file")
    parser.add_argument("-o", "--output", type=str, help="File where to output generated C function calls")

    args = parser.parse_args()

    with open(args.input, "r+") as file:
        plantuml_input = file.read()

    transitions = extract_transitions(plantuml_input)
    for t in transitions:
        t.add_prefix(args.state_prefix)
        # print(t)

    with open(args.output, "w") as file:
        file.write(r"void nfa_generated_register_transitions(nfa_engine_t* nfa)")
        file.write("\n")
        file.write(r"{")
        file.write("\n")
        for transition in transitions:
            call = transition.to_c_func_call()
            if call:
                file.write("    " + transition.to_c_func_call() + "\n")
        file.write("}\n")

if __name__ == "__main__":
    main()
