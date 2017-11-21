class Donation < ApplicationRecord
  belongs_to :user
  belongs_to :proyect

  validates :user_id, :proyect_id, :amount, presence: true
  validates :amount, numericality: {greater_than_or_equal_to: 0}
end
