import argparse
import pathlib
import subprocess


def main():
    parser = argparse.ArgumentParser(description="Run benchmarks over widths and update mixes")
    parser.add_argument("executable")
    parser.add_argument("output")
    parser.add_argument("--metric", choices=["max", "gcd"], default="max")
    parser.add_argument("--n", type=int, default=1000000)
    parser.add_argument("--operations", type=int, default=1000000)
    parser.add_argument("--seed", type=int, default=253)
    parser.add_argument("--runs", type=int, default=5)
    parser.add_argument("--widths", type=int, nargs="+", default=[1, 1024, 1000000])
    parser.add_argument("--updates", type=int, nargs="+", default=[10, 50, 90])
    args = parser.parse_args()
    folder = pathlib.Path(args.output)
    folder.mkdir(parents=True, exist_ok=True)
    for width in args.widths:
        for update in args.updates:
            name = folder / f"{args.metric}-n{args.n}-w{width}-u{update}-seed{args.seed}.csv"
            command = [args.executable, str(args.n), str(args.operations), str(width),
                       str(update), str(args.seed), str(args.runs)]
            if args.metric == "max":
                command += ["random", "time"]
            else:
                command += ["check" if width == 1 else "tree"]
            print(name, flush=True)
            output = subprocess.run(command, capture_output=True, text=True, check=True)
            name.write_text(output.stdout, encoding="utf-8")


if __name__ == "__main__":
    main()
