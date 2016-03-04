var launchClasses = require('./launch_classes');

module.exports = function () {
    this.OSRMBaseLoader = () => new (launchClasses.OSRMBaseLoader.bind(launchClasses.OSRMBaseLoader, this))();

    this._OSRMLoader = () => new (launchClasses._OSRMLoader.bind(launchClasses._OSRMLoader, this))();
}
