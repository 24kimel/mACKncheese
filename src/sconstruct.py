
sources = ["main.c"]
output_name = "test"
env = Environment(parse_flags="-I.")

output = env.Program(target=output_name, source=sources)
