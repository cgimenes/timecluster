import argparse
import pandas as pd
from plotting import run


def load_file(filename, index, column):
    df = pd.read_csv(filename, index_col=index, parse_dates=True)
    series = df[column]
    series = series.head(20)  # TODO REMOVE THIS
    return series


def main(args):
    series = load_file(args.filename, args.index, args.column)
    run(series)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog="TimeCluster", description="Analytical tool for time-series data"
    )
    parser.add_argument("filename", help="Data file path in the CSV format")
    parser.add_argument("-c", "--column", help="CSV data column name", required=True)
    parser.add_argument(
        "-i", "--index", help="CSV index column name", default="datetime"
    )
    args = parser.parse_args()

    main(args)
