'use strict';

module.exports = {
    Location: class {
        constructor (lon, lat) {
            this.lon = lon;
            this.lat = lat;
        }
    },

    _osmStr: class {
        constructor (DB) {
            this.str = null;
            this.DB = DB;
        }

        get xml () {
            if (!this.str) this.str = this.DB.toXML();
            return this.str;
        }
    }
}
