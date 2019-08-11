use std::fmt;

////////////////////////////////////////////////////////////////////////////////
// TODO Separate crate

// TODO: Try tuple struct SortedVec(Vec<T>, Vec<T>)
struct SortedVec<T> {
    data: Vec<T>,
}

impl<T> SortedVec<T> {
    fn new(data: Vec<T>) -> Self {
        return SortedVec::<T> { data: data };
    }
}

impl<T: fmt::Display> fmt::Display for SortedVec<T> {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let mut dump = String::new();
        for v in &self.data {
            dump.push_str(&v.to_string());
            dump.push(',');
        }
        return write!(f, "[{}]", dump);
    }
}

////////////////////////////////////////////////////////////////////////////////

fn main() {
    let v = vec![1, 2, 3];

    let vec = SortedVec::<i32>::new(v);

    println!("Hello SortedVec: {}", vec);
}
