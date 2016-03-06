module.exports = function () {
    this.diffTables = (expectedTable, actual, options, callback) => {
        options = Object.assign({}, {
            missingRow: true,
            surplusRow: true,
            missingCol: true,
            surplusCol: true,
            misplacedCol: true
        }, options);

        var expected = expectedTable.hashes();

        expected.forEach((row, i) => {
            for (var j in row) {
                if (row[j] !== actual[i][j]) return callback(err);
            }
        });

        callback();
    }
}
