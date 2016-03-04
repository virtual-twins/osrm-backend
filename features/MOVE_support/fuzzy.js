// TODO in original, this was a class -- maybe change back

// export function FuzzyMatch () {
//     this.match = (got, want) => {
//         var matchPercent = want.match(/(.*)\s+~(.+)%$/),
//             matchAbs = want.match(/(.*)\s+\+\-(.+)$/),
//             matchRe = want.match(/^\/(.*)\/$/);

//         if (got === want) {
//             return true;
//         } else if (matchPercent) {         // percentage range: 100 ~ 5%
//             var target = parseFloat(matchPercent[1]),
//                 percentage = parseFloat(matchPercent[2]);
//             if (target === 0) {
//                 return true;
//             } else {
//                 var ratio = Math.abs(1 - parseFloat(got) / target);
//                 return 100 * ratio < percentage;
//             }
//         } else if (matchAbs) {             // absolute range: 100 +-5
//             var margin = parseFloat(matchAbs[2]),
//                 fromR = parseFloat(matchAbs[1]) - margin,
//                 toR = parseFloat(matchAbs[1]) + margin;
//             return parseFloat(got) >= fromR && parseFloat(got) <= toR;
//         } else if (matchRe) {               // regex: /a,b,.*/
//             return got.match(matchRe[1]);
//         } else {
//             return false;
//         }
//     }

//     this.matchLocation = (got, want) => {
//         return this.match(got[0], util.format('%d ~0.0025%', want.lat)) &&
//             this.match(got[1], util.format('%d ~0.0025%', want.lon));
//     }
// }
