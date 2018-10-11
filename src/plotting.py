import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns


def create_plot():
    # NORMAL
    normal = pd.read_csv('data/normal.csv')

    # Normal DataFrame
    normal_df = pd.DataFrame(normal)
    # print(normal_df)
    print("Log Normal Distribution")
    print(normal_df['raw'].describe())

    # matplotlib histogram
    plt.figure(figsize=(24, 16))
    plt.title('Random Engine Log Normal Distribution')
    plt.hist(normal['raw'], color='red', edgecolor='black')
    # seaborn histogram
    # sns.distplot(normal_df['raw'], hist=True, kde=False, color='blue', hist_kws={'edgecolor': 'black'})

    # Add labels
    plt.xlabel('Random Int')
    plt.ylabel('Freq.')
    # plt.show()

    # POISSON
    poisson_csv = pd.read_csv('data/poisson.csv')

    # Poisson DataFrame
    poisson_df = pd.DataFrame(poisson_csv)
    print("Poisson Distribution")
    print(poisson_df['raw'].describe())

    plt.figure(figsize=(24, 16))
    plt.title('Random Engine Poisson Distribution')
    plt.hist(poisson_df['raw'], color='orange', edgecolor='black')
    # sns.distplot(poisson_df['raw'], hist=True, kde=False, color='red', hist_kws={'edgecolor': 'black'})
    plt.show()


if __name__ == '__main__':
    print("Plotting from Python")
    create_plot()
